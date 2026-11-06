use std::sync::mpsc;
use std::sync::Arc;
use std::sync::Mutex;
use std::thread;
//use std::process::id;

type Job = Box<dyn FnBox + Send + 'static>;


struct Worker {
    id:     usize,
    thread: thread::JoinHandle<()>,
}

trait FnBox {
    fn call_box(self:  Box<Self>);
}

impl<F: FnOnce()> FnBox for F {
    fn call_box(self: Box<F>) {
        (*self)()
    }
}

impl Worker {
    fn new(id: usize, receiver: Arc<Mutex< mpsc::Receiver<Job> > >) -> Worker {
        let thread = thread::spawn( move || { 
            loop { 
               while let Ok(job) = receiver.lock().unwrap().recv() {

                   println!("Worker {} got a job; executing.", id);
                   // (*job)();
                   job.call_box();
               }
            } 
        });

        Worker { id, thread }
    }
}



// struct Job;
/*
type Job = Box<FnOnce() + Send + `static>;  */


/***************************************************************************

thread::spawn( | | { func(); });  will execute func() at once , it doesn't meet out demand
the worker object take place of that.
Workers in threadpool will keep idle until a new task is comming, he start to handle the given task.

***************************************************************************/
pub struct ThreadPool {
    workers: Vec<Worker>,
    sender:  mpsc::Sender<Job>,
}

// #[warn(unused_doc_comments)]
impl ThreadPool {
    pub fn new(size: usize) -> ThreadPool {
        assert!(size>0);
        /*****************************************************
          a channel is a kind of thing with 

           n senders ( n>=1 )  <-->  1 receiver


         *****************************************************/
        let (sender, receiver) = mpsc::channel();
        let receiver = Arc::new( Mutex::new(receiver) );
        let mut workers = Vec::with_capacity(size);

        for id in  0..size {
            // create some threads and store them in the vector
            //
            // Core Core Core : 
            //         The allocation of n receivers is not allowed.
            workers.push( Worker::new(id, Arc::clone(&receiver) ) );
        }

        ThreadPool { workers, sender }
    }

    pub fn execute<F>(&self, f: F)
    where
        F: FnOnce() + Send + 'static,
    {
        let job = Box::new(f);
        self.sender.send(job).unwrap();
    }
}




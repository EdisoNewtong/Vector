use std::sync::mpsc;
use std::sync::Arc;
use std::sync::Mutex;
use std::thread;



type Job = Box<dyn FnBox + Send + 'static>;

enum Message {
    NewJob(Job),
    Terminate,
}


struct Worker {
    id:     usize,
    thread: Option<thread::JoinHandle<()>>,
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
    fn new(id: usize, receiver: Arc<Mutex<mpsc::Receiver<Message>>>) -> Worker {
        let thread = thread::spawn( move || { 
            loop { 
                let message = receiver.lock().unwrap().recv().unwrap();
                match message {
                    Message::NewJob(job) => {
                        println!("Worker {} got a job; executing.", id);
                        job.call_box();
                    }

                    Message::Terminate => {
                        println!("Worker {} was told to terminate.", id);
                        break;
                    }
                }
            } 
        });

        Worker { id, thread: Some(thread) }
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
    sender:  mpsc::Sender<Message>,
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
        self.sender.send( Message::NewJob(job) ).unwrap();
    }
}

impl Drop for ThreadPool {
    fn drop(&mut self) {
        println!("Sending terminate message to all workders.");

        for _ in &mut self.workers {
            self.sender.send( Message::Terminate ).unwrap();
        }
        println!("Shutting down all worker.");

        for worker in &mut self.workers {
            println!("Shutting down workder {}", worker.id);

            if let Some(thread) = worker.thread.take() {
                thread.join().unwrap();
            }
        }
    }
}




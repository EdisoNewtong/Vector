
import os

def main():
    for i in range(1,101):
        s = str( repr(i).zfill(4) )
        # print(s)
        os.makedirs( s )



if __name__ == "__main__":
    main()
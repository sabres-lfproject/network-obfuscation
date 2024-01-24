for i in range(0,110):
    print ("sudo bash ./observer.sh",i)
for i in range(0,110):
    print ("sudo bash ./server.sh %d 0.5 600"%(i))
for i in range(0,110):
    print ("sudo bash ./client.sh %d 0.5 600"%(i))

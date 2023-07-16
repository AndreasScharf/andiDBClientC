import andiDB
import time
value = andiDB.value("input_values", "RPS 1 pressure")
value.set(1337)
ts = time.time()
while 1:
    b = value.get()
    print(b)
    
print(time.time() - ts)
import andiDB
import time
value = andiDB.value("maschine_values", "Betrieb Anlage")
value.set(1337)
ts = time.time()
for i in range(60):
    b = value.get()
    
print(time.time() - ts)
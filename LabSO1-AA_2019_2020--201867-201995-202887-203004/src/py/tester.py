# Python3 code to demonstrate 
# occurrence frequency using 
# naive method 
import json
import subprocess

# initializing string 
F = open("divina_commedia_ridotta.txt", "r")
Fstats = open("stats.txt", "r")
p = subprocess.run(["./analyzer", "1", "1", "divina_commedia_ridotta.txt"],)

# using naive method to get count 
# counting e 
count = {"padella": "bella"}

for i in F.read():
    if count.get(i) is not None:
        if i != "\n" and i != "'" and i != "\"":
            count[i] = count[i] + 1
    else:
        count[i] = 1

stats = Fstats.read()

def stringToMap(str):
    return dict("{" + str)



# printing result 
print("counted: " + str(count)) 
print("c: " + stats)
# print("map file: ", stringToMap(stats))

statsDict = json.loads("{" + stats)

print("map file: ", json.loads("{" + stats))

C = {x: stats[x] - count[x] for x in stats if x in count}

# for c in count:
#     if count[c] == stats[c]:
#         print("Equal: " + c)
#     else:
#         print("DIFFERENT:" + c)

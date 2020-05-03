# Python3 code to demonstrate 
# occurrence frequency using 
# naive method 
import ast
import subprocess

# initializing string 
#F = open("divina_commedia_ridotta.txt", "r")
p = subprocess.run(["./analyzer", "1", "1", "divina_commedia_ridotta.txt"], stdout = subprocess.PIPE)
print(p.stdout.decode("utf-8"))

# using naive method to get count 
# counting e 
#count = {}

# for i in F.read():
#     if count.get(i) is not None:
#         count[i] = count[i] + 1
#     else:
#         count[i] = 1

# # printing result 
# print ("Count of char occ in file is : "
# 							+ str(count)) 

# def stringToMap(str):
#     return ast.literal_eval("{" + str + "}")


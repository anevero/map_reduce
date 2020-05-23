# This script can be used to generate not very big input files for MapReduce
# program and then check if MapReduce output is correct.

import random

sites = ["bsu.by", "fpmi.bsu.by", "student.bsu.by", "edufpmi.bsu.by",
         "webmail.bsu.by", "elib.bsu.by", "library.bsu.by",
         "acm.bsu.by", "acm.bsu.by/courses/122", "acm.bsu.by/wiki"]

number_of_users = 5000
number_of_entries = 500000
data = [{} for i in range(number_of_users)]

input_file = open("./cmake-build-release/map_in_2", "w+")

for i in range(number_of_entries):
    user = random.randint(0, number_of_users - 1)
    site = random.randint(0, len(sites) - 1)
    time = random.randint(1, 10000)

    if sites[site] not in data[user]:
        data[user][sites[site]] = []
    data[user][sites[site]].append(time)

    input_file.write("%s\t%d\t%d\n" % (sites[site], user, time))

input_file.close()
print("Data has been written to the file!")

while True:
    user_id = int(input("Enter user id to get his data: "))
    if user_id == -1:
        break

    print("User #%d:" % user_id)
    for site in data[user_id].keys():
        average = sum(data[user_id][site]) / len(data[user_id][site])
        minimum = min(data[user_id][site])
        maximum = max(data[user_id][site])
        print("%s\t%d\t%d\t%d" % (site, average, minimum, maximum))

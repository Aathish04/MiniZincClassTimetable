import csv
def decomment(csvfile,commentchar='#'):
    for row in csvfile:
        raw = row.split(commentchar)[0].strip()
        if raw: yield raw

with open("inputdata/Sections.csv") as SectionsFile:
    reader = csv.DictReader(decomment(SectionsFile))
    SectionsList = list(i for i in reader)
    # print(SectionsList)

with open("inputdata/Courses.csv") as CoursesFile:
    reader = csv.DictReader(decomment(CoursesFile))
    CoursesList = list(i for i in reader)
    # print(CoursesList)

with open("inputdata/Faculty.csv") as FacultyFile:
    reader = csv.DictReader(decomment(FacultyFile))
    FacultyList = list(i for i in reader)
    # print(FacultyList)
datadict = {int(key["sectionid"]):[] for key in SectionsList}

for sectionclass in SectionsList:
    for course in CoursesList:
        if course["courseid"] == sectionclass["courseid"]:
            break
    datadict[int(sectionclass["sectionid"])].append(
        [int(sectionclass["courseid"]),int(sectionclass["facultyid"]),int(course["islab"])]
    )

dataarray=[datadict[key] for key in datadict]

outputdict = {
    "sectiondetails":dataarray
}

import json
with open("Temp.json","w") as f:
    json.dump(outputdict,f)
print(dataarray)
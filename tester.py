import subprocess

TESTS_PER_INPUT = 10
FOLDERS = ['2x2', '2x3', '2x4', '3x3']

for folder in FOLDERS:
    f = open('tests/'+folder+'.txt', 'w')
    for test in range(TESTS_PER_INPUT):
        for i in range(1, TESTS_PER_INPUT+1):
            p = subprocess.Popen(["./tp3", "tests/"+str(folder)+"/"+str(i)+"in"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            out, err = p.communicate()
            p.wait()
            f.write(out.decode('utf-8'))
    f.close()
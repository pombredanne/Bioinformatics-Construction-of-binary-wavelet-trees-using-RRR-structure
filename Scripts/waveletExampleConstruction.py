t = "Hajro i Denis imaju 100 bodova!"
abc = sorted(list(set("Hajro i Denis imaju 100 bodova!")))
print(abc)


def binaryTransform(input, start, end):
    print(''.join (input))
    threshold = int(start/2. + end/2.)
    #print("Threshold: {}".format(threshold))
    leftInput = []
    rightInput = []
    for s in input:
        if abc.index(s) <= threshold:
            print(0, end = '')
            leftInput.append(s)
        else:
            print(1, end = '')
            rightInput.append(s)
    print()

    if (start != end):
        binaryTransform(leftInput, start, threshold)
        binaryTransform(rightInput, threshold + 1, end)

binaryTransform(t, 0, len(abc) - 1)
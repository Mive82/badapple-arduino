#!/usr/bin/python3

# Mislav Milinković
#
# Bad Apple arduino project
#
# Python skripta za pretvaranje slika u header file koji se može izravno slati na OLED

from PIL import Image
import sys


def getPixelData(filename):

    with Image.open(filename) as im:
        data = list(im.getdata())

    return data


def invertData(data):
    temp = []
    for x in data:
        temp.append(255 - x)

    return temp


if __name__ == "__main__":

    #folder = "frames/converted"
    filename = sys.argv[1]

    data = getPixelData(filename)
    filename = filename.split('.')[0]
    cleanfilename = ""
    for char in filename:
        if char.isalnum():
            cleanfilename += char

    kod = f"#ifndef {cleanfilename.upper()}_H\n#define {cleanfilename.upper()}_H\n#include <avr/pgmspace.h>\n"
    # kod += "static int dataLocs[] = {\n"

    # for i in data[1]:
    #     kod += str(i)+","
    # kod = kod[:-1]
    # kod += "};"
    kod += f"const unsigned char {cleanfilename.lower()}[" + str(
        len(data) // 8) + "] PROGMEM = {\n"

    # dataOut = open('data1.txt', 'w')
    # dataOut.write(str(data))
    # dataOut.close()

    chunks = [data[x:x + 1024] for x in range(0, len(data), 1024)]

    data.clear()

    for i in chunks:
        row = 0
        col = 127
        currInd = 0
        # print(len(i))
        for j in range(1024):
            #print("J = {}".format(j))
            data.append(i[row * 128 + col])
            #print("Row = {0}, Col = {1}".format(row, col))
            row += 1
            if (row == 8):
                row = 0
                col -= 1

    # dataOut = open('data2.txt', 'w')
    # dataOut.write(str(data2))
    # dataOut.close()

    # print(data2)

    # chunks.reverse()

    # data = []
    # for i in chunks:
    #     for j in i:
    #         data.append(j)

    chunks = [data[x:x + 8] for x in range(0, len(data), 8)]

    # print(chunks)

    data.clear()

    for i in range(len(chunks)):
        for j in range(len(chunks[i])):
            if chunks[i][j] != 0:
                chunks[i][j] = 1

    for i in chunks:
        tempval = 0
        iter = 0
        for j in i:
            tempval |= (j << iter)
            iter += 1
        data.append(tempval)

    # data2 je lista sa bajtovima

    for i in data:
        kod += str(i)
        kod += ","

    # for i in chunks:
    #     kod += "0b"
    #     for j in reversed(i):
    #         if j == 0:
    #             kod += "0"
    #         else:
    #             kod += "1"
    #     kod += ","

    # kod = kod[:-1]
    # kod += "},\n{"

    # for i in data[0]:
    #     kod += str(i) + ","

    kod = kod[:-1]
    kod += "};\n"
    kod += "#endif"

    with open(f"./{cleanfilename.lower()}.h", 'w') as dat:
        dat.write(kod)

    dat.close()

#!/usr/bin/python3

# Mislav Milinković
#
# Bad Apple arduino / RPi project
#
# Enkoder za video

import sys
import os
import shutil
from PIL import Image

# Globalne varijable
width = height = 0


def getDimensions(filename):
    global width
    global height

    with Image.open(filename) as im:
        width, height = im.size
        # print(im.getcolors())
        # if im.mode != "1":
        #     print("Slika nije monokromatska")
        #     exit()

        if width > 128 or height > 128:
            print("Prevelike dimenzije!")
            exit()

        if width != 128 or height != 64:
            print(
                "Video neće raditi na arduinu!\nKako bi radio na arduinu, video mora biti 128x64."
            )

        if height % 16 != 0:
            print("Visina nije višekratnik broja 16")
            exit()
        print(f"{width}x{height}")


# Vraća bitmap pixela
def getPixelData(filename):

    global width
    global height

    with Image.open(filename) as im:
        data = list(im.getdata())
        tempWidth, tempHeight = im.size
        if width != tempWidth or height != tempHeight:
            print(
                f"GREŠKA!!\nFrameovi nisu istih dimenzija!\nDobio {tempWidth}x{tempHeight}, očekivao {width}x{height}\nNa datoteci {filename}.\nIzlazim..."
            )
            exit()

    return data


def writeHeader(directory, filenames, outFileName, loopNum: int):
    byte_num = 0
    outFile = open(outFileName, 'ab')
    getDimensions(f"{directory}/{filenames[0]}")
    writebuffer = [width, height // 8]  # 2
    for byte in writebuffer:
        outFile.write(byte.to_bytes(1, byteorder='big'))
        byte_num += 1
    outFile.write(len(filenames).to_bytes(2, 'big'))  # 2
    byte_num += 2
    header_str = str.encode("baduino")
    outFile.write(header_str)
    byte_num += len(header_str)
    outFile.write(loopNum.to_bytes(1, 'big'))
    byte_num += 1
    for i in range(510 - byte_num):
        outFile.write((0).to_bytes(1, 'big'))
    outFile.write(0xF0.to_bytes(1, 'big'))
    outFile.write(0x0D.to_bytes(1, 'big'))

    return


def writeFrames(data, outFileName):

    outFile = open(outFileName, 'ab')

    for byte in data:
        outFile.write(byte.to_bytes(1, byteorder='big'))
    return


def processFrames(directory, filenames, outFileName):
    #getDimensions(f"{directory}/{filenames[0]}")
    progressNum = 0
    totalNum = len(filenames)
    for filename in filenames:
        progressNum += 1
        progressPerc = int((progressNum / totalNum) * 100)
        print('\rObradujem frame {}/{} [{}%]'.format(progressNum, totalNum,
                                                     progressPerc),
              end="")

        x = f"{directory}/{filename}"
        data = getPixelData(x)

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

        chunks = [data[x:x + 8] for x in range(0, len(data), 8)]

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

        writeFrames(data, outFileName)

    return


if __name__ == "__main__":
    # argumenti: <source directory> <output file name>
    argumenti = sys.argv

    # Provjeravanje argumenata
    if len(argumenti) < 3:
        print("Argumenti nisu dobri")
        exit()

    if len(argumenti) < 4:
        loopNum = 0
    else:
        loopNum = int(argumenti[3])

    directory = argumenti[1]
    outFileName = argumenti[2]

    # Provjeravanje i enumeriranje ulaznog direktorija
    if not (os.path.isdir(directory)):
        print("Ulazni direktorij {} ne postoji.".format(
            directory.split("./")[1]))
        exit()

    filenames = os.listdir(directory)
    if len(filenames) == 0:
        print(
            "Nije pronaden ni jedan frame u direktoriju {}.\nIzlazim.".format(
                directory.split("./")[1]))
        exit()

    filenames.sort()

    print("Pronadeno {} frameova.".format(len(filenames)))

    if os.path.isfile(outFileName):
        print("Datoteka '{}' postoji, brišem ju.".format(outFileName))
        os.remove(outFileName)

    if (loopNum != 0):
        print("Video će se ponavljati.")
    writeHeader(directory, filenames, outFileName, loopNum)
    processFrames(directory, filenames, outFileName)
    print("")

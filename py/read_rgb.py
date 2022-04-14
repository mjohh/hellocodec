from PIL import Image

im = Image.open('room.jpg')
pix = im.load()
width = im.size[0]
height = im.size[1]

f = open('room.txt', 'w')
f.write("{},{}\n".format(width, height))
for w in range(width):
    for h in range(height):
        r,g,b = pix[w,h]
        f.write('{},{},{}\n'.format(r,g,b))
f.close()

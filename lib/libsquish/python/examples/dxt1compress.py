#!/usr/bin/env python
import sys
import pygame
import squish

if __name__ == '__main__':

    if len(sys.argv) != 2:
        print 'Usage: python dxt1compress.py <image>'
        sys.exit(1)

    pygame.init()
    fn = sys.argv[1]
    img = pygame.image.load(fn)
    width, height = img.get_size()

    # ensure RGBA
    if img.get_bytesize() != 4:
        img = img.convert(32)
    data = pygame.image.tostring(img, 'RGBA', False)

    # convert
    img_dxt1 = squish.compressImage(data, width, height, squish.DXT1)

    # write
    fn_dxt1 = fn.rsplit('.', 1)[0] + '.dxt1'
    with open(fn_dxt1, 'wb') as fd:
        fd.write(img_dxt1)

    print 'Written to', fn_dxt1

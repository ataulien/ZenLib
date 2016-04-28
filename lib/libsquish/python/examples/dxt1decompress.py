#!/usr/bin/env python
import sys
import pygame
import squish

if __name__ == '__main__':

    if len(sys.argv) != 4:
        print 'Usage: python dxt1decompress.py <image.dxt1> <width> <height>'
        sys.exit(1)

    fn, width, height = sys.argv[1:]
    width, height = int(width), int(height)
    with open(fn, 'rb') as fd:
        data = fd.read()

    # convert
    img_raw = squish.decompressImage(data, width, height, squish.DXT1)

    # write
    fn_png = fn.rsplit('.', 1)[0] + '.png'
    img = pygame.image.fromstring(img_raw, (width, height), 'RGBA')
    pygame.image.save(img, fn_png)

    print 'Written to', fn_png

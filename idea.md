1. header
2. fileheader
3. img data
    + PNG raw
	+ BMP remove file header, 32 bit with alpha, double heighth add binary and mask
	
	
## ico header

size|data
---|---
2|0
2|1 // ico or cur
2|file number = 7

**files:**
*  256x256 as PNG
* 64x64 as BMP
* 48x48 as BMP 
* 40x40 as BMP
* 32x32 as BMP
* 24x24 as BMP
* 16x16 as BMP

## ico file header
 
 size|data
 ---|---
 1|width (0 == 256)
 1|heigth (0 == 256)
 1|0 // numbers in color pallet
 1|0
 2|1 // hotspot in cur format
 2|32 // bitsPPx hotspot in cur 
 4|img size in byte
 4|offset to data
 
## BMP info header

size|data
---|---
4|40 // size 
4|width
4|heigth * 2 (for AND Mask)
2|1 // numbetr color planes
2|32 // bitsPPx
4|0 // compression (no)
4|0 // img size (0 == auto only for comp == 0)
4|0 // resolution don't care
4|0 // resolution don't care
4|0 // color palelt size (0 == all colors)
4|0 // importen colors??

1. header
2. img Data
3. AND Mask 1 bit opacity

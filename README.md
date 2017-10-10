# LazuriteXBee
XBee AT mode emulation for [Lazurite 920J](http://www.lapis-semi.com/lazurite-jp/products/lazurite-920j)

## Feature
- XBee AT mode emulation
- AT commands support

## Supported AT Commands

If there is no arguments, the command returns the parameter value. Otherwise, it sets the parameter and returns comments.

- +++  : enter to command mode (wait for 3sec after "+++" is sent)
- AT   : returns OK
- ATBD : change baudrate
- ATCH : change channel
- ATID : change PAN ID
- ATDT : change destination address
- ATDR : change data rate
- ATPL : change power level
- ATCN : finish command mode
- ATWR : write current parameters to flash memory
- ATRE : reset Lazurite

See the details of parameter [here](http://www.lapis-semi.com/lazurite-jp/contents/reference/subghz/subghz.html)

## Dependencies
- [Lazurite IDE](http://www.lapis-semi.com/lazurite-jp/download)

## License
MIT

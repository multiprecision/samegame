# SameGame

A clone of SameGame with configurable size, color, and target.

![](/documentation/screenshot_1.png?raw=true)
![](/documentation/screenshot_2.png?raw=true)
![](/documentation/screenshot_3.png?raw=true)

## Prerequisites

To run the compiled binaries, you should have Microsoft Visual C++ 2013 Redistributeable installed.

## Compiling

To build from source, open the project file with Microsoft Visual C++ 2013. This project utilizes SFML 2.1 and Boost Library.

## Configuring

You can configure the game by editing `config.xml` located in `binaries` directory. If the file is unreadable or non-existent, the program will continue using the default configuration and attempt to create or overwrite the configuration file.

## Configuring `config.xml`

### `column_size`

The column size. Valid range is 2 to 128. Default value is 10.

### `row_size`

The row size. Valid range is 2 to 70. Default value is 10.

### `block_width`

The block width in pixel. Valid range is 2 to 640. Default value is 32.

### `block_height`

The block width in pixel. Valid range is 2 to 640. Default value is 32.

### `block_needed`

 The number of horizontally or vertically adjoined blocks of the same color needed to remove them. Valid range is 2 to 128. Default value is 3.

### `color`

The block color in HTML color format (e.g. #FF0000, red).

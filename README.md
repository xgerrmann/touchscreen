## Arduino Serial
#If not yet installed
sudo apt-get install screen

#Connect to Arduino Serial via:
screen /dev/ttyUSB0 9600

#Close the serial connection via:
ctrl+a
\
y

###Function keybinding in VIM
## Make + upload: 
# Place in .vimrc:
:map <F2> :!make upload <CR>
## Make + upload + Serial communication terminal (with automatic closing)
:map <F2> :!screen -X -S arduino kill; make upload && gnome-terminal -e 'screen -S arduino /dev/ttyUSB0 9600' <CR>

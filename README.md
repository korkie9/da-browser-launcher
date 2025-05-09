# Da Browser Launcher

Da Browser Launcher lets users launch their preferred browser in their specific profiles.

For instance, should a user wish to open Firefox with the profile name "John", instead of going to `about:profiles` in their browser and searching for their profile manually, they can merely launch Da Browser Launcher and select their profile directly.

## Supported Browsers
#### NOTE: App does not yet check if browser is installed. It will assume it is and run the appropriate script eg. 'firefox -P username'
- Firefox
- Brave
- Chrome
- Zen (NOTE: For Zen to launch, user will need to have Zen installed with [flatpak](https://flathub.org/apps/app.zen_browser.zen)

## Dependancies
- raylib
- sqlite

## Running/Installing from source
- clone repository with ``git clone https://github.com/korkie9/da-browser-launcher.git``
- ``cd da-browser-launcher``
- Run: ``make``
- run ``./da-browser-launcher`` from root

OR

- clone repository with ``git clone https://github.com/korkie9/da-browser-launcher.git``
- ``cd da-browser-launcher``
- Install: makepkg -si
- Run: da-browser-launcher


# Installing via package manager

### Arch
`yay -S da-browser-launcher`


## Database
The SQLite database is stored at:  
`~/.config/da-browser-launcher/database.db`  

To reset your profiles, delete this file.

## Resetting
- run ``make clean``


## Uninstalling
### Remove the package
sudo pacman -Rns da-browser-launcher

### Remove user database
rm -rf ~/.config/da-browser-launcher/



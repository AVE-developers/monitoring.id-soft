# monitoring.id-soft



#### Мониторинг транспорта накладных



>Примерный порядок установки:


```bash
sudo apt install --yes git-core
mkdir ~/job
cd ~/job
git clone https://github.com/AVE-developers/monitoring.id-soft.git
cd /tmp
php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');"
php ./composer-setup.php --install-dir=~/job --filename=composer.phar
php -r "unlink('composer-setup.php');"
cd ~/job/monitoring.id-soft
php ~/job/composer.phar update
php -f ./run.php help
```

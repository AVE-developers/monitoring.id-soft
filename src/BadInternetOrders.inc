<?php
/**
 * Created by PhpStorm.
 * User: Администратор
 * Date: 26.07.2017
 * Time: 22:06
 */

namespace AveWd\Console\Command;

use AveWd\myZIP;use Carbon\Carbon;use Symfony\Component\Console\Application;use Symfony\Component\Console\Input\InputDefinition;use Symfony\Component\Console\Input\InputInterface;use Symfony\Component\Console\Input\InputOption;use Symfony\Component\Console\Output\OutputInterface;

class BadInternetOrders extends Command {

    /**
     * Конструктор класса
     */protected function configure()
    {
        parent::configure();
        $this->setDescription('Проверка не отправленных на аптеку интернет-заказов');
        $this->setHelp('Здесь будет описание комманды' . PHP_EOL . PHP_EOL);
        $this->setDefinition(new InputDefinition([
            new InputOption('email','e',InputOption::VALUE_IS_ARRAY | InputOption::VALUE_REQUIRED,
                'Адрес электронной почты для отправки результата'),
            new InputOption('only-work-time', 'w', InputOption::VALUE_NONE | InputOption::VALUE_OPTIONAL,
                'Работать только в рабочее время с 9-18 с понедельника по пятницу'),
        ]));
    }

    /**
    * Проверка опций, первичные настройки
    * @param \Symfony\Component\Console\Input\InputInterface $input
    * @param \Symfony\Component\Console\Output\OutputInterface $output
    */protected function interact(InputInterface $input, OutputInterface $output)
    {
        parent::interact($input,$output);
        if($input->getOption('only-work-time'))
        if(\Carbon\Carbon::now()->isWeekday())
        if(\Carbon\Carbon::now()->hour<9 || \Carbon\Carbon::now()->hour>18){
            die('Не работаем в нерабочее время'.str_repeat(PHP_EOL,2));
        }
        if($input->getOption('verbose'))\R::fancyDebug(true);
    }



    /**
    * Инициализацция драйвера PDO
    * @param \Symfony\Component\Console\Input\InputInterface $input
    * @param \Symfony\Component\Console\Output\OutputInterface $output
    */protected function initialize(InputInterface $input, OutputInterface $output)
    {
        parent::initialize($input,$output);
        $output->writeln('Готовим PDO');
        $this->_pg__ave_skd =   new \PDO($this->config->database->ave_skd->dsn.' connect_timeout=3 options=\'--client_encoding=UTF-8\'',
                                $this->config->database->ave_skd->username, $this->config->database->ave_skd->password);
        $this->_pg__repl =      new \PDO($this->config->database->transport->dsn.' connect_timeout=3 options=\'--client_encoding=WIN1251\'',
                                $this->config->database->transport->username, $this->config->database->transport->password);
        $output->writeln('Настраиваем драйвер БД');
        // Генерировать исключение при ошибках
        $this->_pg__ave_skd->setAttribute(\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
        $this->_pg__repl->setAttribute(\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
        // Преобразовать пустые строки в NULL
        $this->_pg__ave_skd->setAttribute(\PDO::ATTR_ORACLE_NULLS, \PDO::NULL_EMPTY_STRING);
        $this->_pg__repl->setAttribute(\PDO::ATTR_ORACLE_NULLS, \PDO::NULL_EMPTY_STRING);
        // Преобразовывать числовые значения в строки во время выборки. Значение типа bool.
        $this->_pg__ave_skd->setAttribute(\PDO::ATTR_STRINGIFY_FETCHES, false);
        $this->_pg__repl->setAttribute(\PDO::ATTR_STRINGIFY_FETCHES, false);
        // Режим выборки по умолчанию
        $this->_pg__ave_skd->setAttribute(\PDO::ATTR_DEFAULT_FETCH_MODE, \PDO::FETCH_NAMED);
        $this->_pg__repl->setAttribute(\PDO::ATTR_DEFAULT_FETCH_MODE, \PDO::FETCH_NAMED);
    }

    /**
    * Основной код команды
    * @param \Symfony\Component\Console\Input\InputInterface $input
    * @param \Symfony\Component\Console\Output\OutputInterface $output
    */protected function execute(InputInterface $input, OutputInterface $output)
    {
        parent::execute($input, $output);

        /** @var $ave_db \PDOStatement */

        $output->write('Запрос в 1С, выбираем плохие интенет-заказы: -> ');
        try {
            $this->_pg__ave_skd = $this->_pg__ave_skd->query(file_get_contents(__DIR__ . '/../sql/pg/getBadIntenetOrders.sql'));
        } catch (PDOException  $e) {
            die("Ошибка: " . $e);
        }
        if(!$this->_pg__ave_skd) die('Запрос выполнить не удалось'.PHP_EOL);
        if(!$this->_pg__ave_skd->rowCount()) die('0 стр.'.PHP_EOL);
        $output->writeln($this->_pg__ave_skd->rowCount().' стр.');

        $this->_pg__ave_skd->fetchAll(\PDO::FETCH_FUNC,function(){
            $prihod = \R::dispense('prihod');
            $prihod->n_doc = func_get_arg(0);
            $prihod->d_doc = func_get_arg(1);
            $prihod->n_apt = func_get_arg(2);
            $prihod->n_intzak = func_get_arg(3);
            \R::store($prihod);
        });unset($this->_pg__ave_skd);
        $mddoc = Carbon::createFromTimestamp(
            \R::getCell('SELECT min(d_doc) FROM prihod GROUP BY d_doc'),'Europe/Moscow')
            ->subDay()->startOfDay();

        $output->writeln('Готовим запрос в базу репликации');
        try {
            $sql = (new \Twig_Environment(new \Twig_Loader_Filesystem(realpath(__DIR__ . '/../sql/pg/twig')), [
                'cache' => realpath(__DIR__ . '/../cache/'),
                'auto_reload' => true,
                'optimizations' => (int)false,
            ]))->render('getTrasportErrors.sql.twig',
                [
                    'ndocs' => \R::getCol('SELECT DISTINCT n_doc FROM prihod ORDER BY n_doc'),
                    'mddoc' => $mddoc,
                    'idapts' => \R::getCol('SELECT DISTINCT n_apt FROM prihod ORDER BY n_apt'),
                ]
            );
        } catch (\Twig_Error_Syntax $e) {
            $output->writeln('<err>' . $e->getSourceContext()->getPath() . '</err>');
            $output->writeln('<info>Строка #' . $e->getTemplateLine() . ': ' . $e->getRawMessage() . '</info>');
        }
        myZIP::$zPath = __DIR__ . '/../tmp/sql.'.\Carbon\Carbon::now()->timestamp.'.zip';
        myZIP::$fPath = __DIR__ . '/../tmp/getInternetOrdersTrasportErrors.sql';
        myZIP::$password = strtoupper(crc32($sql));
        file_put_contents(myZIP::$fPath,$sql);myZIP::doZip();

        // Делаем запрос в базу репликации
        $output->write('Делаем запрос в базу репликации: -> ');
        try {
            $this->_pg__repl = $this->_pg__repl->query($sql);
        } catch
        (\Exception $e) {
            die("Ошибка: " . $e);
        }
        if(!$this->_pg__repl) die('Запрос выполнить не удалось'.PHP_EOL);
        if(!$this->_pg__repl->rowCount()) die('0 стр.'.PHP_EOL);
        $output->writeln($this->_pg__repl->rowCount().' стр.');

        $this->_pg__repl->fetchAll(\PDO::FETCH_FUNC,function(){
            $prihod = \R::dispense('insupp');
            $prihod->ndoc = func_get_arg(0);
            $prihod->idapt = func_get_arg(1);
            $prihod->tname = func_get_arg(2);
            $prihod->npack = func_get_arg(3);
            $prihod->data = func_get_arg(4);
            $prihod->enqueue = func_get_arg(5);
            $prihod->msg = func_get_arg(6);
            \R::store($prihod);
        });unset($this->_pg__repl);

        $docs = \R::getAssoc('SELECT * FROM prihod a INNER JOIN insupp b ON a.n_doc=b.ndoc');
        $html = (new \Twig_Environment(new \Twig_Loader_Filesystem(realpath(__DIR__ . '/../templates/'))))
                ->render('mail_bad_internet_orders.twig',['docs' => $docs,'Carbon'=>Carbon::now('Europe/Moscow')]);
        unset($docs);

        if ($input->getOption('email')) {
            $mail = (new \PHPMailer);
            file_put_contents(__DIR__.'/../tmp/mail_bad_internet_orders.html',
                $mail->msgHTML($html));
            $mail->setLanguage('ru');
            $mail->CharSet = 'utf-8';
            $mail->setFrom('transport@monitoring.id-soft', 'Монитор транспорта накладных');
            $mail->clearReplyTos();

            foreach ($input->getOption('email') as $address){
                $mail->addAddress($address);
                $mail->addReplyTo($address);
            }
            $mail->Subject = 'Мониторинг интернет-заказов';
            if (file_exists(myZIP::$zPath)) $mail->addAttachment(myZIP::$zPath);
            $mail->addEmbeddedImage(__DIR__ . '/../img/filter-internet-orders-img.jpg',
                'filter-internet-orders-img', 'filter-internet-orders-img.jpg');
            if (!$mail->send()) die('Ошибка отправки: '.$mail->ErrorInfo);
            else $output->writeln('Message has been sent');
            }
        }
    }


/** @var Application $app */
$app->add(new BadInternetOrders('check-internet-orders'));
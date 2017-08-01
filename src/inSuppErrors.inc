<?php

namespace AveWd\Console\Command;

use AveWd\myZIP;
use Carbon\Carbon;
use Symfony\Component\Console\Application;
use Symfony\Component\Console\Input\InputDefinition;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Input\InputOption;
use Symfony\Component\Console\Output\OutputInterface;

class inSuppErrors extends Command
{

    protected function configure()
    {
        parent::configure();
        $this->setDescription('Проверка не отправленных на аптеку приходов ( кроме интернет-заказов )');
        $this->setHelp('Здесь будет описание комманды' . PHP_EOL . PHP_EOL);
    }

    protected function interact(InputInterface $input, OutputInterface $output)
    {
        parent::interact($input, $output);
    }

    protected function initialize(InputInterface $input, OutputInterface $output)
    {
        parent::initialize($input, $output);
    }

    protected function execute(InputInterface $input, OutputInterface $output)
    {
        parent::execute($input, $output);

        /** @var $ave_db \PDOStatement */

        $output->write('Запрос в 1С, выбираем плохие приходы: -> ');
        try {
            $this->_pg__ave_skd = $this->_pg__ave_skd->query(
                file_get_contents(__DIR__ . '/../sql/pg/getBadSuppErrors.sql'));
        } catch (PDOException  $e) {
            die("Ошибка: " . $e . PHP_EOL);
        }
        if (!$this->_pg__ave_skd) die('Запрос выполнить не удалось' . PHP_EOL);
        if (!$this->_pg__ave_skd->rowCount()) die('0 стр.' . PHP_EOL);
        $output->writeln($this->_pg__ave_skd->rowCount() . ' стр.');

        $this->_pg__ave_skd->fetchAll(\PDO::FETCH_FUNC, function () {
            $prihod            = \R::dispense('prihod');
            $prihod->n_doc     = func_get_arg(0);
            $prihod->d_doc     = func_get_arg(1);
            $prihod->n_apt     = func_get_arg(2);
            $prihod->nd_post   = func_get_arg(3);
            $prihod->nd_post_kd = func_get_arg(4);
            $prihod->n_post    = func_get_arg(5);
            $prihod->n_post_kd  = func_get_arg(6);
            \R::store($prihod);
        });
        unset($this->_pg__ave_skd);
        $mddoc = Carbon::createFromTimestamp(
            \R::getCell('SELECT min(d_doc) FROM prihod GROUP BY d_doc'), 'Europe/Moscow')
            ->subDay()->startOfDay();

        $output->writeln('Готовим запрос в базу репликации');
        try {
            $sql = (new \Twig_Environment(new \Twig_Loader_Filesystem(realpath(__DIR__ . '/../sql/pg/twig')), [
                'auto_reload'   => true,
                'optimizations' => (int)false,
            ]))->render('getTrasportErrors.sql.twig',
                [
                    'ndocs'  => \R::getCol('SELECT DISTINCT n_doc FROM prihod ORDER BY n_doc'),
                    'mddoc'  => $mddoc,
                    'idapts' => \R::getCol('SELECT DISTINCT n_apt FROM prihod ORDER BY n_apt'),
                ]
            );
        } catch (\Twig_Error_Syntax $e) {
            $output->writeln('<err>' . $e->getSourceContext()->getPath() . '</err>');
            $output->writeln('<info>Строка #' . $e->getTemplateLine() . ': ' . $e->getRawMessage() . '</info>');
        }
        myZIP::$zPath    = __DIR__ . '/../tmp/sql.' . Carbon::now()->timestamp . '.zip';
        myZIP::$fPath    = __DIR__ . '/../tmp/getNormalOrdersTrasportErrors.sql';
        myZIP::$password = strtoupper(dechex(crc32($sql)));
        file_put_contents(myZIP::$fPath, $sql);
        myZIP::doZip();

        // Делаем запрос в базу репликации
        $output->write('Делаем запрос в базу репликации: -> ');
        try {
            $this->_pg__repl = $this->_pg__repl->query($sql);
        } catch
        (\Exception $e) {
            die("Ошибка: " . $e);
        }
        if (!$this->_pg__repl) die('Запрос выполнить не удалось' . PHP_EOL);
        if (!$this->_pg__repl->rowCount()) die('0 стр.' . PHP_EOL);
        $output->writeln($this->_pg__repl->rowCount() . ' стр.');

        $this->_pg__repl->fetchAll(\PDO::FETCH_FUNC, function () {
            $prihod          = \R::dispense('insupp');
            $prihod->ndoc    = func_get_arg(0);
            $prihod->idapt   = func_get_arg(1);
            $prihod->tname   = func_get_arg(2);
            $prihod->npack   = func_get_arg(3);
            $prihod->data    = func_get_arg(4);
            $prihod->enqueue = func_get_arg(5);
            $prihod->msg     = func_get_arg(6);
            $prihod->inactive  = func_get_arg(7);
            \R::store($prihod);
        });
        unset($this->_pg__repl);
        //$a = \R::getAll('SELECT * FROM prihod');
        //$b = \R::getAll('SELECT * FROM insupp');
        $docs = \R::getAll('SELECT * FROM prihod a LEFT OUTER JOIN insupp b ON a.n_doc=b.ndoc ORDER BY a.n_apt ASC, a.n_doc ASC ');
        $html = (new \Twig_Environment(new \Twig_Loader_Filesystem(realpath(__DIR__ . '/../templates/'))))
            ->render('mail_bad_normal_orders.twig', ['docs'   => $docs,
                                                     'Carbon' => [
                                                         Carbon::now('Europe/Moscow'),
                                                         Carbon::now('Europe/Moscow'),
                                                         Carbon::now('Europe/Moscow')]]);
        unset($docs);

        if ($input->getOption('email')) {
            $mail = new \PHPMailer();
            file_put_contents(__DIR__ . '/../tmp/mail_bad_normal_orders.html',
                $mail->msgHTML($html));
            $mail->setLanguage('ru');
            $mail->CharSet = 'utf-8';
            $mail->setFrom('transport@monitoring.id-soft', 'Монитор транспорта накладных');
            $mail->clearReplyTos();

            foreach ($input->getOption('email') as $address) {
                $mail->addAddress($address);
                $mail->addReplyTo($address);
            }
            $mail->Subject = 'Мониторинг приходных накладных';
            if (file_exists(myZIP::$zPath)) $mail->addAttachment(myZIP::$zPath);
            $mail->addEmbeddedImage(__DIR__ . '/../img/filter-normal-orders-img.jpg',
                'filter-normal-orders-img', 'filter-normal-orders-img.jpg');
            if (!$mail->send()) die('Ошибка отправки: ' . $mail->ErrorInfo);
            else $output->writeln('Message has been sent');
        }
    }
}

/** @var Application $app */
$app->add(new inSuppErrors('check-normal-orders'));
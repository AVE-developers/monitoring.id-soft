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

        $docs = \R::getAll('SELECT * 
                                FROM prihod a 
                                LEFT OUTER JOIN insupp b ON a.n_doc=b.ndoc
                                LEFT OUTER JOIN aptek c ON a.n_apt=c.id_instance
                                ORDER BY a.n_apt ASC, a.n_doc ASC');

        $html = (new \Twig_Environment(new \Twig_Loader_Filesystem(realpath(__DIR__ . '/../templates/'))))
            ->render('mail_bad_normal_orders.twig', ['docs'   => $docs,
                                                     'Carbon' => [
                                                         Carbon::now('Europe/Moscow'),
                                                         Carbon::now('Europe/Moscow'),
                                                         Carbon::now('Europe/Moscow')]]);



        if ($input->getOption('email')) {
            $mail = new \PHPMailer();
            file_put_contents(__DIR__ . '/../tmp/mail_bad_normal_orders.html', $mail->msgHTML($html));
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

/** @var \Symfony\Component\Console\Application $app */
$app->add(new inSuppErrors('check-normal-orders'));
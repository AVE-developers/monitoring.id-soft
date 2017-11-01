<?php
namespace AveWd\Console\Command;

use Symfony\Component\Console\Input\InputDefinition;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Input\InputOption;
use Symfony\Component\Console\Output\OutputInterface;
use Symfony\Component\Yaml\Yaml;

class bad_internet_price extends \Symfony\Component\Console\Command\Command
{
    protected function configure()
    {
        parent::configure();
        $this->setDescription('Проверка цен интернет-партий');
        $this->setHelp(
            implode(PHP_EOL,
                [
                   'Отправляем письмо по указанным в команде адресам.'
                ]
            )
        );
        $this->mailSubject = 'Мониторинг цен интернет-партий';
        $config = implode(
            DIRECTORY_SEPARATOR, [
                __DIR__, '..', 'etc', 'config.yml',
            ]
        );
        if (file_exists($config)) {
            $this->config = Yaml::parse(
                file_get_contents(
                    realpath($config)
                ),
                Yaml::PARSE_OBJECT_FOR_MAP
            );
        } else {
            die('Не найден ' . $config);
        }
        $this->setDefinition(
            new InputDefinition(
                [
                    new InputOption(
                        'email',
                        'e',
                        InputOption::VALUE_IS_ARRAY | InputOption::VALUE_REQUIRED,
                        'Адрес электронной почты для отправки результата'
                    ),
                    new InputOption(
                        'only-work-time',
                        'w',
                        InputOption::VALUE_NONE,
                        'Работать только в рабочее время с 9-20'
                    ),
                ]
            )
        );
    }

    protected function initialize(InputInterface $input, OutputInterface $output)
    {
        parent::initialize($input, $output);

        $arr_itr_Options = (new \ArrayObject(
            $this->getDefinition()->getOptions())
        )->getIterator();
        while ($arr_itr_Options->valid()) {
            if ($this->getDefinition()->getOption(
                $arr_itr_Options->current()->getName()
            )->isValueRequired()) {
                if (!$input->getOption(
                    $arr_itr_Options->current()->getName()
                )) {
                    die(
                        implode(
                            PHP_EOL,
                            [
                                'Не верные параметры команды',
                                'Не указан: ' . $arr_itr_Options->current()->getDescription(),
                            ]
                        ) . str_repeat(PHP_EOL, 2)
                    );
                }
            }
            $arr_itr_Options->next();
        }

        if ($input->getOption('only-work-time')
            && (
                Carbon::now()->hour < 9
                || Carbon::now()->hour > 20
            )
        ) {
            die('Не работаем в нерабочее время' . str_repeat(PHP_EOL, 2));
        }

        if ($input->getOption('verbose'))
            \R::fancyDebug(true);

        static $PDOtryToConnect = 5;
        try {
            $output->writeln('Готовим PDO: осталось попыток: ' . $PDOtryToConnect);
            $pg_options = [
                \PDO::ATTR_ERRMODE => \PDO::ERRMODE_EXCEPTION,
                \PDO::ATTR_ORACLE_NULLS => \PDO::NULL_EMPTY_STRING,
                \PDO::ATTR_STRINGIFY_FETCHES => false,
                \PDO::ATTR_DEFAULT_FETCH_MODE => \PDO::FETCH_NAMED,
            ];
            $this->skd = new \PDO(
                $this->config->database->ave_skd->dsn . ' connect_timeout=3 options=\'--client_encoding=UTF-8\'',
                $this->config->database->ave_skd->username,
                $this->config->database->ave_skd->password,
                $pg_options
            );
//            $this->repl = new \PDO(
//                $this->config->database->transport->dsn . ' connect_timeout=3 options=\'--client_encoding=WIN1251\'',
//                $this->config->database->transport->username,
//                $this->config->database->transport->password,
//                $pg_options
//            );
            $output->writeln('Настраиваем драйвер БД');
            $PDOtryToConnect = 5;
        } catch (\Throwable $throwable) {
            if (--$PDOtryToConnect) {
                $this->interact($input, $output);
            } else {
                die($throwable);
            }
        }
    }

    protected function execute(InputInterface $input, OutputInterface $output)
    {
        $Images = new \ArrayObject([], \ArrayObject::ARRAY_AS_PROPS | \ArrayObject::STD_PROP_LIST);
        $Images->filter = new \SplFileInfo(
            implode(
                DIRECTORY_SEPARATOR,
                [
                    __DIR__,
                    '..',
                    'img',
                    (new \ReflectionClass($this))->getShortName() . '.jpg',
                ]
            )
        );
        $html = (new \Twig_Environment(
            new \Twig_Loader_Filesystem(
                realpath(
                    implode(
                        DIRECTORY_SEPARATOR,
                        [__DIR__, '..', 'templates']
                    )
                )
            )
        ))->render(
            (new \ReflectionClass($this))->getShortName() . '.twig',
            [
                'docs' => $this->getPrice(),
                'Images' => $Images
            ]
        );
        if ($input->getOption('email')) {
            $mail = new \PHPMailer(true);
            $mail->isSMTP();
            $mail->Hostname = 'monitoring.id-soft';
            $mail->Host = $this->config->smtp->server;
            $mail->SMTPSecure = 'tls';
            $mail->Port = $this->config->smtp->port;
            $mail->SMTPDebug = 0;
            $mail->SMTPAuth = true;
            $mail->Debugoutput = function ($mess) use ($output) {
                $output->write($mess);
            };
            $mail->Username = $this->config->smtp->username;
            $mail->Password = $this->config->smtp->password;
            file_put_contents(
                implode(
                    DIRECTORY_SEPARATOR, [
                        sys_get_temp_dir(),
                        (new \ReflectionClass($this))->getShortName() . '.html',
                    ]
                ),
                $mail->msgHTML($html)
            );
            $mail->setLanguage('ru');
            $mail->CharSet = 'utf-8';
            $mail->setFrom(
                $this->config->smtp->username,
                'Монитор транспорта накладных'
            );
            $mail->clearReplyTos();

            foreach ($input->getOption('email') as $address) {
                $mail->addAddress($address);
                $mail->addReplyTo($address);
            }
            $mail->Subject = $this->mailSubject;
//            if (file_exists(myZIP::$zPath)) {
//                $mail->addAttachment(myZIP::$zPath);
//            }
            foreach ($Images as $image) {
                if ($image instanceof \SplFileInfo) {
                    $mail->addEmbeddedImage(
                        $image->getRealPath(),
                        $image->getFilename(),
                        $image->getFilename()
                    );
                }
            }
            try {
                $mail->send();
            } catch (\phpmailerException $e) {
                $output->writeln($mail->ErrorInfo);
                die($e);
            } catch (\Throwable $e) {
                die($e);
            } finally {
                $output->writeln('Сообщение отправлено');
            }
        }
    }

    protected function getPrice(): \ArrayObject
    {
        try {
            /** @var \PDOStatement $price */
            $price = $this->skd->query(
                file_get_contents(
                    implode(
                        DIRECTORY_SEPARATOR,
                        [
                            __DIR__,
                            '..',
                            'sql',
                            'pg',
                            (new \ReflectionClass($this))->getShortName() . '.sql',
                        ]
                    )
                )
            );
        } catch (\Throwable $throwable) {
            die($throwable);
        } catch (\PDOException $PDOException){
            die($PDOException);
        } finally {
            if ($price->rowCount()){
                return new \ArrayObject($price->fetchAll(),\ArrayObject::STD_PROP_LIST|\ArrayObject::ARRAY_AS_PROPS);
            } else {
                exit(204);
            }
        }
    }

}
/** @global \Symfony\Component\Console\Application $app */
$app->add(new bad_internet_price('bad-internet-price'));
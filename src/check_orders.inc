<?php

namespace AveWd\Console\Command;

use AveWd\myZIP;
use Carbon\Carbon;
use Symfony\Component\Console\Input\InputDefinition;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Input\InputOption;
use Symfony\Component\Console\Output\OutputInterface;
use Symfony\Component\Yaml\Yaml;

class check_orders extends \Symfony\Component\Console\Command\Command
{
    protected $config;

    protected function configure()
    {
        parent::configure();
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
            $this->repl = new \PDO(
                $this->config->database->transport->dsn . ' connect_timeout=3 options=\'--client_encoding=WIN1251\'',
                $this->config->database->transport->username,
                $this->config->database->transport->password,
                $pg_options
            );
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

        $this->getPrihod();
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
                'docs' => \R::getAll(
                    'SELECT * 
                                FROM prihod a 
                                LEFT OUTER JOIN insupp b ON a.n_doc=b.ndoc
                                LEFT OUTER JOIN aptek c ON a.n_apt=c.id_instance
                                ORDER BY a.n_apt ASC, a.n_doc ASC'
                ),
                'Carbon' => [
                    Carbon::now('Europe/Moscow'),
                    Carbon::now('Europe/Moscow'),
                    Carbon::now('Europe/Moscow'),
                ],
            ]
        );
        if ($input->getOption('email')) {
            $mail = new \PHPMailer();
            $mail->isMail();
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
                'transport@monitoring.id-soft',
                'Монитор транспорта накладных'
            );
            $mail->clearReplyTos();

            foreach ($input->getOption('email') as $address) {
                $mail->addAddress($address);
                $mail->addReplyTo($address);
            }
            $mail->Subject = $this->mailSubject;
            if (file_exists(myZIP::$zPath)) {
                $mail->addAttachment(myZIP::$zPath);
            }
            $mail->addEmbeddedImage(
                implode(
                    DIRECTORY_SEPARATOR, [
                        __DIR__,
                        '..',
                        'img',
                        'filter-' . basename(get_called_class()) . '.jpg',
                    ]
                ),
                'filter-' . (new \ReflectionClass($this))->getShortName() . '-img',
                'filter-' . (new \ReflectionClass($this))->getShortName() . '.jpg'
            );
            if (!$mail->send()) die('Ошибка отправки: ' . $mail->ErrorInfo);
            else $output->writeln('Сообщение отправлено');
        }
    }

    protected function getPrihod(): void
    {
        try {
            $skd = $this->skd->query(
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

            $skd->setFetchMode(
                \PDO::FETCH_INTO,
                \R::dispense('prihod')
            );
            while ($bean = $skd->fetch(\PDO::FETCH_INTO)) {
                /** @var \RedBeanPHP\OODBBean $bean */
                \R::store($bean);
                $skd->setFetchMode(
                    \PDO::FETCH_INTO,
                    \R::dispense('prihod')
                );
            }

        } catch (\Throwable $throwable) {
            die($throwable);
        } finally {
            $this->getAptek();
            $this->getPackets();
        }
    }

    protected function getAptek(): void
    {
        $sql = file_get_contents(
            implode(
                DIRECTORY_SEPARATOR,
                [
                    __DIR__,
                    '..',
                    'sql',
                    'pg',
                    __FUNCTION__ . '.sql',
                ]
            )
        );
        /** @var \PDOStatement $repl */
        $repl = $this->repl->query($sql);
        $repl->setFetchMode(
            \PDO::FETCH_INTO,
            \R::dispense('aptek')
        );
        while ($bean = $repl->fetch(\PDO::FETCH_INTO)) {
            /** @var \RedBeanPHP\OODBBean $bean */
            \R::store($bean);
            $repl->setFetchMode(
                \PDO::FETCH_INTO,
                \R::dispense('aptek')
            );
        }
    }

    protected function getPackets(): void
    {
        try {
            $sql = (
            new \Twig_Environment(
                new \Twig_Loader_Filesystem(
                    realpath(
                        implode(
                            DIRECTORY_SEPARATOR,
                            [
                                __DIR__,
                                '..',
                                'sql',
                                'pg',
                                'twig',
                            ]
                        )
                    )
                ),
                [
                    'auto_reload' => true,
                    'optimizations' => 0,
                ])
            )->render('getPackets.sql.twig',
                    [
                        'ndocs' => \R::getCol('SELECT DISTINCT n_doc FROM prihod ORDER BY n_doc'),
                        'mddoc' => Carbon::createFromTimestamp(
                            \R::getCell('SELECT min(d_doc) FROM prihod GROUP BY d_doc'),
                            'Europe/Moscow'
                        )->subDay()->startOfDay(),
                    ]
                );
            /** @var \PDOStatement $repl */
            $repl = $this->repl->query($sql);

            $repl->setFetchMode(
                \PDO::FETCH_INTO,
                \R::dispense('insupp')
            );
            while ($bean = $repl->fetch(\PDO::FETCH_INTO)) {
                /** @var \RedBeanPHP\OODBBean $bean */
                \R::store($bean);
                $repl->setFetchMode(
                    \PDO::FETCH_INTO,
                    \R::dispense('insupp')
                );
            }

        } catch (\Twig_Error_Syntax $exception) {
            die(implode(
                null,
                [
                    $exception->getSourceContext()->getPath(),
                    '#',
                    $exception->getTemplateLine(),
                    ': ',
                    $exception->getRawMessage(),
                ]
            ));
        } catch (\Throwable $throwable) {
            die($throwable);
        } finally {
            myZIP::$zPath = implode(
                DIRECTORY_SEPARATOR, [
                    sys_get_temp_dir(),
                    'sql.' . microtime(true) . '.zip',
                ]
            );
            myZIP::$fPath = implode(
                DIRECTORY_SEPARATOR, [
                    sys_get_temp_dir(),
                    (new \ReflectionClass($this))->getShortName() . '.sql',
                ]
            );
            myZIP::$password = strtoupper(
                dechex(
                    crc32($sql)
                )
            );
            if (file_put_contents(myZIP::$fPath, $sql)) {
                myZIP::doZip();
            }
        }
    }
}
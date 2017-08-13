<?php

namespace AveWd;

abstract class myZIP
{
    public static $fPath, $zPath, $password;

    public static function doZip(): void
    {
        $proc = proc_open(self::command(), [
            0 => ['pipe', 'r'],
            1 => ['pipe', 'w'],
            2 => ['pipe', 'w']], $pipes);
        if (is_resource($proc) && proc_close($proc)) {
            print_r(
                stream_get_contents(
                    $pipes[2]
                )
            );
        }
    }

    public static function command(): string
    {
        return escapeshellcmd(
            implode(' ',
                [
                    '7z',
                    'a',
                    self::$zPath,
                    self::$fPath,
                    '-p' . escapeshellarg(self::$password),
                ]
            )
        );
    }

}
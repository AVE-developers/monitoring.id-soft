<?php

namespace AveWd;

abstract class myZIP {
    public static $fPath, $zPath, $password;
    public static function doZip(?string $file_path=null, ?string $zip_path=null, ?string $password=null) : void {
        if(!is_null($file_path))self::$fPath=$file_path;
        if(!is_null($zip_path))self::$zPath=$zip_path;
        if(!is_null($password))self::$password=$password;
        ob_start();
        system(self::command(), $ret);
        if (!$ret) {
            ob_end_clean();
        } else {
            ob_end_flush();
        }
    }
    public static function command () : string {
        return '7za a ' . self::$zPath . ' ' . self::$fPath . ' -p' . escapeshellarg(self::$password);
    }
}
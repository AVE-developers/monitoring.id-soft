WITH orders AS (
    SELECT
      public._document7084._number :: INTEGER        AS ndoc,
      -- Номер документа
      public._document7084._date_time :: TIMESTAMP   AS ddoc,
      -- Дата документа
      public._reference22._code :: SMALLINT          AS idapt,
      public._document7084._Fld9649 :: VARCHAR(20)    AS int_ord_num
    -- Код аптеки
    FROM public._document7084
      LEFT JOIN public._reference22
      -- местаХранения
        ON public._reference22._idrref = public._document7084._Fld7261RRef
    WHERE public._document7084._fld7268rref = E'\\x80B17C208209698D4D847B33CBB4670B'
          -- ТипНакладной: Интернет-заказ (_enum2660)
          AND public._document7084._marked = FALSE -- Не помеченные на удаление
          AND public._document7084._date_time < now() - '1 hours' :: INTERVAL
          -- Документы, созданные более часа назад
          AND public._document7084._Fld7264RRef = E'\\x82CBD37640B3337E4C90036947A739E4'
          -- СтатусОфиса: Выгружен в аптеку (_enum7097)
          AND (public._document7084._Fld7265RRef = E'\\x8A38CB71495F580F4D46B293AA0ADE7C'
               -- СтатусАптеки: Неопределено (_Enum111)
               OR public._document7084._Fld7265RRef =
                  E'\\x00000000000000000000000000000000') -- СтатусАптеки: ПустоеЗначение (_Enum111)
    ORDER BY public._document7084._date_time DESC
)
SELECT
  a.ndoc :: INTEGER                                      AS ndoc,
  extract(epoch from a.ddoc at time zone 'Europe/Moscow')AS ddoc,
  a.idapt :: SMALLINT                                    AS idapt,
  a.int_ord_num :: VARCHAR(20) AS int_ord_num
FROM orders a
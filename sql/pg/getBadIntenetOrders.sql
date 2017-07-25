WITH orders AS (
    SELECT
      public._document7084._number :: INTEGER        AS ndoc,
      -- Номер документа
      public._document7084._date_time :: TIMESTAMPTZ AS ddoc,
      -- Дата документа
      public._reference22._code :: SMALLINT          AS idapt
    -- Код аптеки
    FROM public._document7084
      LEFT JOIN public._reference22
      -- местаХранения
        ON public._reference22._idrref = public._document7084._Fld7261RRef
    WHERE public._document7084._fld7268rref = E'\\x80B17C208209698D4D847B33CBB4670B'
          -- ТипНакладной: Интернет-заказ (_enum2660)
          AND public._document7084._date_time < now() - '1 hours' :: INTERVAL
          -- Документы, созданные более часа назад
          AND public._document7084._Fld7264RRef = E'\\x82CBD37640B3337E4C90036947A739E4'
          -- СтатусОфиса: Выгружен в аптеку (_enum7097)
          AND (public._document7084._Fld7265RRef = E'\\x8A38CB71495F580F4D46B293AA0ADE7C'
               -- СтатусАптеки: Неопределено (_Enum111)
               OR public._document7084._Fld7265RRef =
                  E'\\x00000000000000000000000000000000') -- СтатусАптеки: ПустоеЗначение (_Enum111)
)
SELECT
  a.ndoc :: INTEGER                                      AS ndoc,
  a.ddoc :: TIMESTAMPTZ                                  AS ddoc,
  a.idapt :: SMALLINT                                    AS idapt,
  EXTRACT(YEAR FROM a.ddoc) :: SMALLINT                  AS ddoc_year,
  EXTRACT(MONTH FROM a.ddoc) :: SMALLINT                 AS ddoc_month,
  EXTRACT(DAY FROM a.ddoc) :: SMALLINT                   AS ddoc_day,
  EXTRACT(HOUR FROM a.ddoc) :: SMALLINT                  AS ddoc_hour,
  EXTRACT(MINUTE FROM a.ddoc) :: SMALLINT                AS ddoc_minute,

  EXTRACT(YEAR FROM c.mddoc :: TIMESTAMPTZ) :: SMALLINT  AS mddoc_year,
  EXTRACT(MONTH FROM c.mddoc :: TIMESTAMPTZ) :: SMALLINT AS mddoc_month,
  EXTRACT(DAY FROM c.mddoc :: TIMESTAMPTZ) :: SMALLINT   AS mddoc_day

FROM orders a
  LEFT JOIN (SELECT min(b.ddoc) :: TIMESTAMPTZ AS mddoc
             FROM orders b) c ON a.ddoc = c.mddoc
ORDER BY a.ndoc ASC
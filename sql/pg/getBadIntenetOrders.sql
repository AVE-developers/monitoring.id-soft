SELECT
  prihod._number :: INTEGER                                                    AS НомерПрихода,
  -- Номер документа Документ.УЗ_ПоступлениеТовара
  extract(EPOCH FROM prihod._date_time AT TIME ZONE 'Europe/Moscow') :: BIGINT AS ДатаПрихода,
  -- Дата документа Документ.УЗ_ПоступлениеТовара
  apteki._code :: SMALLINT                                                     AS НомерАптеки,
  -- Код аптеки Справочник.МестаХранения
  prihod._Fld9649                                                              AS НомерИнтернетЗаказа
-- Номер интернет-заказа

FROM public._document7084 prihod
  LEFT OUTER JOIN public._reference22 apteki
  -- Справочник.МестаХранения
    ON prihod._Fld7261RRef = apteki._idrref

WHERE prihod._fld7268rref = E'\\x80B17C208209698D4D847B33CBB4670B'
      -- ТипНакладной: Интернет-заказ (_enum2660)
      AND prihod._marked = FALSE -- Не помеченные на удаление
      AND prihod._date_time < now() - '1 hours' :: INTERVAL
      -- Документы, созданные более часа назад
      AND prihod._Fld7264RRef = E'\\x82CBD37640B3337E4C90036947A739E4'
      -- СтатусОфиса: Выгружен в аптеку (_enum7097)
      AND (prihod._Fld7265RRef = E'\\x8A38CB71495F580F4D46B293AA0ADE7C'
           -- СтатусАптеки: Неопределено (_Enum111)
           OR prihod._Fld7265RRef =
              E'\\x00000000000000000000000000000000'
        -- СтатусАптеки: ПустоеЗначение (_Enum111)
      )
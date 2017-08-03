SELECT
  prihod._number :: INTEGER                                                    AS n_doc,
  -- Номер документа Документ.УЗ_ПоступлениеТовара
  extract(EPOCH FROM prihod._date_time AT TIME ZONE 'Europe/Moscow') :: BIGINT AS d_doc,
  -- Дата документа Документ.УЗ_ПоступлениеТовара
  apteki._code :: SMALLINT                                                     AS n_apt,
  -- Код аптеки Справочник.МестаХранения
  prihod._Fld7249                                                              AS nd_post,
  -- ВхНомерНакл	Документ.УЗ_ПоступлениеТовара.Реквизит.ВхНомерНакл
  zepochkiKD._Fld9295                                                          AS nd_post_kd,
  -- НомерВхПоставщика	РегистрСведений.ЦепочкиПоКроссДокингу.Ресурс.НомерВхПоставщика
  postavshik._description                                                      AS n_post,
  -- Поставщик Наименовние
  postavshikKD._description                                                    AS n_post_kd
-- Поставщик Наименовние

FROM public._document7084 prihod

  LEFT JOIN public._reference22 apteki
  -- Справочник.МестаХранения
    ON apteki._idrref = prihod._Fld7261RRef

  LEFT JOIN public._inforg9292 zepochkiKD
  -- цепочки по кросдокингу РегистрСведений.ЦепочкиПоКроссДокингу
    ON prihod._idrref = zepochkiKD._fld9293rref

  LEFT JOIN public._reference33 postavshik
  -- Поставщик Справочник.Поставщики
    ON prihod._fld7259rref = postavshik._idrref

  LEFT JOIN public._reference33 postavshikKD
  -- ПоставщикКД Справочник.Поставщики
    ON prihod._fld9648rref = postavshikKD._idrref


WHERE prihod._fld7268rref != E'\\x80B17C208209698D4D847B33CBB4670B'
      -- ТипНакладной: Интернет-заказ (_enum2660)
      AND prihod._marked = FALSE
      -- Не помеченные на удаление
      AND prihod._date_time < now() - '1 hours' :: INTERVAL
      -- Документы, созданные более часа назад
      AND prihod._date_time > now() - '720 hours' :: INTERVAL
      -- Документы, моложе 720 часов
      AND prihod._Fld7264RRef = E'\\x82CBD37640B3337E4C90036947A739E4'
      -- СтатусОфиса: Выгружен в аптеку (_enum7097)
      AND (prihod._Fld7265RRef = E'\\x8A38CB71495F580F4D46B293AA0ADE7C'
           -- cтатус документа аптеки: Неопределено (_Enum111)
           OR prihod._Fld7265RRef = E'\\x00000000000000000000000000000000'
        -- cтатус документа аптеки: ПустоеЗначение (_Enum111)
      )

ORDER BY prihod._date_time DESC

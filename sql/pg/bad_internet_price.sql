SELECT
  zen._Fld7903 :: BIGINT                                                   AS ПартияКод,
  zen._Fld7904 :: DECIMAL(10, 2)                                           AS Цена,
  zen._Fld7905 :: INTEGER                                                  AS Коэффициент,
  (zen._Fld7904 / zen._Fld7905) :: DECIMAL(10, 3)                          AS ЦенаЗаШтуку,
  (trunc(zen._Fld7904 / zen._Fld7905, 2)) :: DECIMAL(10, 2)                AS ПравильнаяЦенаЗаШтуку,
  (trunc(zen._Fld7904 / zen._Fld7905, 2) * zen._Fld7905) :: DECIMAL(10, 2) AS ПравильнаяЦенаЗаПартию


FROM ave_skd.public._InfoRg7900 AS zen
WHERE
  zen._Fld7906 > now() - '30 days' :: INTERVAL
  AND zen._Fld7905 > 1
  AND ((zen._Fld7904 / zen._Fld7905) :: FLOAT - trunc(zen._Fld7904 / zen._Fld7905, 2) :: FLOAT) :: FLOAT != 0
  AND zen._fld7908_type = E'\\x08'
  AND zen._fld7908_rtref = E'\\x00001BAC'
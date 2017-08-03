SELECT
  tab.id_instance :: SMALLINT         AS id_instance,
  stat.id_instance :: BOOLEAN NOTNULL AS inactive,
  tab.is_active :: BOOLEAN            AS is_active,
  tab.is_closed :: BOOLEAN            AS is_closed
FROM repl.fo_list tab
  LEFT JOIN repl.fo_inactive_stat stat ON tab.id_instance = stat.id_instance
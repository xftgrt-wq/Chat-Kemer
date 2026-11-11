# Анализ Atlas Client 2.6.2

Переданный `net.atlasclient.atlaspe.apk` просмотрен как ZIP/APK-контейнер.

- В APK обнаружены ресурсы `res/layout/quickmenu_button.xml` и `res/layout/quickmenus_dialog.xml`.
- В APK нет каталога `lib/` с нативными библиотеками `.so`.
- В ресурсах/строках присутствует упоминание `Hotbar Shortcuts`.

Вывод: APK Atlas — полезный визуальный ориентир, но не готовый нативный модуль для переименования в Chat Kemer. Проект Chat Kemer реализуется отдельно через публичный SDK Levi/Preloader; закрытый код Atlas не копируется.

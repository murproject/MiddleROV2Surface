# Прошивка для пульта MiddleROV

![MiddleROV](https://robocenter.net/media/images/1_ZcZXJNe.max-1000x500.jpg)

Данная прошивка предназначена **для пульта управления** из набора MiddleROV. Пульт представляет собой чёрный "ящик" и разъёмами, и уже к нему подключается сам аппарат.

> Если нужна прошивка для платы **на самом аппарате**, то вам нужен [MiddleRov2OnBoard](https://github.com/murproject/MiddleRov2OnBoard).

Для прошивки, подключите USB-кабель к пульту и воспользуйтесь [Arduino IDE](https://www.arduino.cc/en/Main/Software), выбрав нужный последовательный порт и плату `Arduino Leonardo`.

## Обратите внимание

Требуется [библиотека `PsxNewLib`](https://github.com/SukkoPera/PsxNewLib). Она доступна в менеджере библиотек Arduino IDE (`Скетч / Покдлючить библиотеку / Управлять библиотеками`, далее вводим в поиск `PsxNewLib` и устанавливаем, соглашаясь и на установку зависимостей).

>module Main where
>import GameApi.GameApi

>main :: IO ()
>main = do env <- createEnv
>          everyapi <- getEveryApi env
>          mlapi <- getMainLoopApi everyapi
>          initWindow mlapi 800 600

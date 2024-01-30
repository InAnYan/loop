module CheckerMonad where

import Control.Monad

newtype Checker e = Checker { runCheck }

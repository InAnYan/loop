module S where

import System.Environment
import Control.Monad


data Error = Error SourcePosition ErrorKind

data ErrorKind = ErrorKind String

data SemanticContext = SemanticContext [Definition] Int

data Definition = Definition String DefinitionKind SourcePosition

data DefinitionKind = VarDef | FuncDef | ClassDef

newtype State s a = State { runState :: s -> (a, s) }

instance Functor (State s) where
  fmap = liftM

instance Applicative (State s) where
  pure = return
  (<*>) = ap
  
instance Monad (State s) where
  return x = State ( \ s -> (x, s) )
  p >>= k = q where
    p' = runState p        -- p' :: s -> (a, s)
    k' = runState . k      -- k' :: a -> s -> (b, s)
    q' s0 = (y, s2) where  -- q' :: s -> (b, s)
        (x, s1) = p' s0    -- (x, s1) :: (a, s)
        (y, s2) = k' x s1  -- (y, s2) :: (b, s)
    q = State q'

type SemanticCheck = State SemanticContext [Error]

checkStmt :: Stmt -> SemanticCheck
checkStmt (Stmt pos (VarDecl export name initExpr)) = do
    e1 <- checkMaybeExpr initExpr
    e2 <- checkExport pos export
    e3 <- define pos name
    return $ e1 ++ e2 ++ e3
checkStmt _ = undefined

checkMaybeExpr :: Maybe Expr -> SemanticCheck
checkMaybeExpr (Just e) = checkExpr e
checkMaybeExpr Nothing = return []

checkExpr :: Expr -> SemanticCheck
checkExpr = undefined

checkExport = undefined
define = undefined

module SemanticCheck where

data Env = Env (Maybe Env) EnvKind [Def]

data EnvKind = ModuleEnv | FuncEnv | BlockEnv | ClassEnv

data Def = Def SrcPos String

type SemanticCheker = SrcPos -> Env -> ([Error], Env)

type Error = WithPos ErrorKind

data ErrorKind
  = UndefinedRef String
  | Redefinition String
  | ExportDisallowed String

checkModule :: Module -> SemanticChecker
checkModule (Module stmts) = checkStmts stmts

checkStmts :: [Stmt] -> SemanticChecker
checkStmts = doChecksMany checkStmt

checkStmt :: Stmt -> SemanticChecker
checkStmt (Stmt stmtPos kind) _ = checkStmtKind kind stmtPos

checkMaybeStmt :: (Maybe Stmt) -> SemanticChecker
checkMaybeStmt = checkMaybe checkStmt

checkStmtKind :: StmtKind -> SemanticChecker
checkStmtKind (DeclStmt decl) = checkDecl decl
checkStmtKind (ExprStmt expr) = checkExpr expr
checkStmtKind (PrintStmt expr) = checkExpr expr
checkStmtKind (ReturnStmt expr) =
  doChecks [ checkReturnAllowed
          , checkMaybeExpr expr
          , checkClassReturn expr
          ]
checkStmtKind (IfStmt cond thenArm elseArm) =
  doChecks [ checkExpr cond
          , checkStmt thenArm
          , checkMaybeStmt elseArm
          ]
checkStmtKind (WhileStmt cond body) =
  doChecks [ checkExpr cond
          , checkStmt body
          ]
checkStmtKind (ImportAsStmt path name) =
  doChecks [ checkImportedModule path
          , define name
          ]
checkStmtKind (ImportFromStmt names path) =
  doChecks [ checkImportedModule path
          , defineMany names
          ]
checkStmtKind (BlockStmt stmts) =
  doChecks [ beginEnv BlockEnv
           , checkStmts stmts
           , endEnv
           ]
  
checkDecl :: Decl -> SemanticChecker
checkDecl (Decl export name kind) =
  doChecks [ checkExport export name
           , define name
           , checkDeclKind kind
           ]

checkDeclKind :: DeclKind -> SemanticChecker
checkDeclKind (VarDecl init) = checkMaybeExpr init
checkDeclKind (FuncDecl sign) = checkFuncSign sign
checkDeclKind (ClassDecl methods) =
  doChecks [ beginEnv ClassEnv
           , checkFuncs methods
           , endEnv
           ]

checkExport :: Bool -> String -> SemanticChecker
checkExport export name pos env =
  if envIsGlobal env == export
  then ([], env)
  else ([(Error pos (ExportDisallowed name))], env)

checkExpr :: Expr -> SemanticChecker
checkExpr (Expr exprPos kind) _ = checkExprKind kind exprPos

checkMaybeExpr :: (Maybe Expr) -> SemanticChecker
checkMaybeExpr = checkMaybe checkExpr

checkExprKind :: ExprKind -> SemanticChecker
checkExprKind (BinExpr _ left right) = checkExpr right <!> checkExpr left
checkExprKind (UnaryExpr _ expr) = checkExpr expr
checkExprKind (CallExpr calee args) =
  checkExpr calee <!> doChecksMany checkExpr args

define :: Ident -> SemanticChecker
define name pos env =
  if envHasSelf name env
  then ([(Error pos (Redefinition name))], env)
  else ([], (envDefine name pos env))

defineMany :: [Ident] -> SemanticChecker
defineMany = doChecksMany define

envDefine :: Ident -> SrcPos -> Env -> Env
envDefine name pos (Env parent kind defs) =
  Env parent kind ((Def pos name):defs)

noErrors :: SemanticChecker
noErrors = modifyEnv id

modifyEnv :: (Env -> Env) -> SemanticChecker
modifyEnv fn _ env = ([], (fn env))

error :: ErrorKind -> SemanticChecker
error kind pos env = ([(Error pos kind)], env)

checkMaybe :: (a -> SemanticChecker) -> (Maybe a) -> SemanticChecker
checkMaybe fn (Just x) = fn x
checkMaybe _ _ = noErrors

doChecks :: [SemanticChecker] -> SemanticChecker
doChecks = fold1 combineChecks noErrors

doChecksMany :: (a -> SemanticChecker) -> [a] -> SemanticChecker
doChecksMany fn lst = doChecks (map fn lst)

(<!>) :: SemanticChecker -> SemanticChecker -> SemanticChecker
(<!>) ch1 ch2 pos env0 = (e1 ++ e2, env2)
  where
    (e1, env1) = ch1 pos env0
    (e2, env2) = ch2 pos env1

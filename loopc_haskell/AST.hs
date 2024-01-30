module AST where

type Path = String

data File = File Path String

type Line = Int

data SrcPos = SrcPos File Line

data Module = Module [Stmt]

data WithPos a = WithPos SrcPos a

type Expr = WithPos ExprKind
type Stmt = WithPos StmtKind

type Ident = String

data StmtKind
  = DeclStmt Decl
  | ExprStmt Expr
  | ReturnStmt (Maybe Expr)
  | IfStmt Expr Stmt (Maybe Stmt)
  | WhileStmt Expr Stmt
  | PrintStmt Expr
  | ImportAsStmt Path Ident
  | ImportFromStmt [Ident] Path
  | BlockStmt [Stmt]

data Decl = Decl Bool Ident DeclKind

data DeclKind
  = VarDecl (Maybe Expr)
  | FuncDecl FuncSign
  | ClassDecl [Func]

data FuncSign = FuncSign [String] Stmt
data Func = Func Ident FuncSign

data ExprKind
  = BinExpr BinOp Expr Expr
  | UnaryExpr UnaryOp Expr
  | CallExpr Expr [Expr]
  | VarExpr Ident
  | IntExpr Int
  | BoolExpr Bool
  | NullExpr
  | DictExpr [DictEntry]
  | StrExpr String
  | GetItemExpr Expr [Expr]
  | GetAttrExpr Expr Ident

data DictEntry = DictEntry Expr Expr

data BinOp
  = Add | Subtract | Multiply | Divide
  | Greater | Less | GreaterEqual | LessEqual
  | Equal | NotEqual
  | LogicalAnd | LogicalOr

data UnaryOp = Plus | Negate | LogicalNot


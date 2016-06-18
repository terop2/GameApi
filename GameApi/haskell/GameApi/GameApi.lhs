>{-# LANGUAGE TypeOperators, ForeignFunctionInterface #-}
>module GameApi.GameApi where
>import Foreign.Ptr
>import Foreign.ForeignPtr
>import Foreign.C.Types
>import Foreign.C.String

>newtype BM = BM Int
>newtype CP = CP Int
>newtype DR = DR Int
>newtype OM = OM Int
>newtype FO = FO Int
>newtype WV = WV Int
>newtype BMA = BMA Int
>newtype VBM = VBM Int
>newtype BB = BB Int
>newtype FB = FB Int
>newtype CBM = CBM Int
>newtype SP = SP Int
>newtype SA = SA Int
>newtype PT = PT Int
>newtype V = V Int
>data Vector4 a = Vector4 a a a a
>data (g :*: f) a = Matrix { cells :: g (f a) }
>newtype M = M ((Vector4 :*: Vector4) Float)
>newtype LN = LN Int
>newtype RM = RM Int
>newtype IS = IS Int
>newtype O = O Int
>newtype P = P Int
>newtype OO = OO Int
>newtype PP = PP Int
>newtype PA = PA Int
>newtype LA = LA Int
>newtype TN = TN Int
>newtype SH = SH Int
>newtype C = C Int
>newtype CO = CO Int
>newtype F = F Int
>newtype FA = FA Int
>newtype Ft = Ft Int
>newtype VF = VF Int
>newtype PPT = PPT Int
>newtype PSP = PSP Int
>newtype MA = MA Int
>newtype S = S Int
>newtype Vb = Vb Int
>newtype T = T Int
>newtype E = E Int
>newtype L = L Int
>newtype LAY = LAY Int
>newtype MP = MP Int
>newtype MV = MV Int
>newtype LL = LL Int
>newtype ME = ME Int
>newtype ST = ST Int
>newtype TY = TY Int
>newtype PS = PS Int
>newtype ID = ID Int
>newtype Str = Str Int
>newtype Fi = Fi Int
>newtype Op = Op Int
>newtype D = D Int
>newtype Ht = Ht Int
>newtype BF = BF Int

>data C_BitmapApi
>data C_EveryApi
>data C_Env
>data C_MainLoopApi
>type BitmapApi = Ptr C_BitmapApi
>type EveryApi = Ptr C_EveryApi
>type MainLoopApi = Ptr C_MainLoopApi
>type Env = Ptr C_Env

class EnvOperations bm a where
   find :: Env -> bm -> IO (ForeignPtr a)
   add  :: Env -> ForeignPtr a -> IO bm

>foreign import ccall "GameApi.h createEnv" createEnv :: IO Env
>foreign import ccall "GameApi.h getBitmapApi" getbitmapApi :: EveryApi -> BitmapApi
>foreign import ccall "GameApi.h newbitmap" newbitmap :: BitmapApi -> CInt -> CInt -> CUInt -> IO BM
>foreign import ccall "GameApi.h loadbitmap" loadbitmap_ :: BitmapApi -> CString -> IO BM
>foreign import ccall "GameApi.h init_window" initWindow :: MainLoopApi -> CInt -> CInt -> IO ()
>foreign import ccall "GameApi.h getmainloopapi" getMainLoopApi :: EveryApi -> IO MainLoopApi
>foreign import ccall "GameApi.h geteveryApi" getEveryApi :: Env -> IO EveryApi

>loadbitmap :: BitmapApi -> String -> IO BM
>loadbitmap api s = withCString s $ \ cstr -> loadbitmap_ api cstr

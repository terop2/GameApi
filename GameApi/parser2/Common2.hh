
void P_cb(void *data)
{
  std::cout << "P_cb" << std::endl;
  P_script *script = (P_script*)data;
  script->Prepare2();
}

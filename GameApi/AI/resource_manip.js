
function get_vox_base(num)
{
    var tuple = voxtable[num];
    var base = tuple["base"];
    return base;
}
function get_vox_files(num)
{
    var tuple = voxtable[num];
    var files = tuple["files"];
    return files;    
}
function get_vox_license(num)
{
    var tuple = voxtable[num];
    var license = tuple["license"];
    return license;    
}
function get_file_count(num)
{
    var files = get_vox_files(num);
    retun files.length;
}
function get_file(num,file_num)
{
    var files = get_vox_files(num);
    return files[file_num];
}
function get_url(num,file_num)
{
    return get_vox_base(num) + get_file(num,file_num);
}

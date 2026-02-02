

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
function get_entry_count(a)
{
    return voxtable.length;
}
function get_file_count(num)
{
    var files = get_vox_files(num);
    return files.length;
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

function find_keywords(str)
{
    let str0 = str.replace(/.vox/,'');
    let str1 = str0.replace(/chr_/,'');
    let str2 = str1.replace(/alien_/,'');
    let str3 = str2.replace(/env_/,'');
    let str4 = str3.replace(/mob_/,'');
    let str5 = str4.replace(/obj_/,'');
    let str6 = str5.replace(/scene_/,'');
    let str7 = str6.replace(/veh_/,'');
    let str8 = str7.replace(/\d+/g, '');
    return str8;
}


function set_file(file)
{
    document.getElementById("code").innerText = Files[file];
    document.getElementById("warningg").innerText = Warning[file];
    for (element of Files.keys())
    {
	document.getElementById(element).style.backgroundColor = "transparent";
    }
    document.getElementById(file).style.backgroundColor = "gray";
}


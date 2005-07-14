function get_answer()
{
	show_dialog(0);
	local a = dialog_answer();
	clear_answers();
	return a;
}

function add_answers(...)
{
	for(local i = 0; i < vargc; i++)
		add_answer(vargv[i]);
}
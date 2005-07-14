function get_answer()
{
	show_dialog(0);
	return dialog_answer();
}

function add_answers(...)
{
	for(local i = 0; i < vargc; i++)
		add_answer(vargv[i]);
}
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

add_question("Hello. Agree?");
add_answers("Yes.", "No.");
get_answer();
end_dialog();




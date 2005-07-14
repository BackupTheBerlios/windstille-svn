function intro()
{
	bob_seen_before = true;
	add_question("Hey, my name's bob.  Are you new around here?"); add_answers("Yes, I am.", "Go away"); 
	if (!get_answer())
	{
		ask_questions();
	}
	else
	{
		bye();
	}
}

function hello_again()
{
	add_question("Hello again")
	add_answers("Hey.", "Bye");
	if (!get_answer())
		ask_questions();
	else
		bye();
}

function ask_questions()
{
	add_question("So what are you doing here?");
	add_answers("I work as a mercenary.", "Not much");
	if (!get_answer())
	{
		bob_knows_your_mercenary = true;
		offer_job();
	}
	else
		bye();
}

function offer_job()
{
	add_question("Are you looking for some work?");
	add_answers("I might be, if the offer is good enough.", "No thanks.");
	if (!get_answer())
	{
		bob_knows_job = true;
		explain_job();
	}
	else
		bye();
}

function explain_job()
{
	add_question("Your job is to kill the spider at the end of the level ;)");
	get_answer();
}

function bye()
{
	if (bob_knows_job)
		add_question("Feel free to come back if you need a job");
	else
		add_question("See you again some time");
	get_answer();
}

if (bob_knows_job)
	explain_job();
else if (bob_knows_your_mercenary)
	offer_job();
else if (bob_seen_before)
	hello_again();
else
	intro();

end_dialog();
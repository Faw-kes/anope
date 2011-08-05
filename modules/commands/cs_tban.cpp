/* cs_tban.c - Bans the user for a given length of time
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Based on the original module by Rob <rob@anope.org>
 * Included in the Anope module pack since Anope 1.7.8
 * Anope Coder: Rob <rob@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Send bug reports to the Anope Coder instead of the module
 * author, because any changes since the inclusion into anope
 * are not supported by the original author.
 */
/*************************************************************************/

#include "module.h"

static Module *me;

class TempBan : public CallBack
{
 private:
	dynamic_reference<Channel> chan;
	Anope::string mask;

 public:
	TempBan(time_t seconds, Channel *c, const Anope::string &banmask) : CallBack(me, seconds), chan(c), mask(banmask) { }

	void Tick(time_t ctime)
	{
		if (chan && chan->ci)
			chan->RemoveMode(NULL, CMODE_BAN, mask);
	}
};

static bool CanBanUser(CommandSource &source, Channel *c, User *u2)
{
	User *u = source.u;
	ChannelInfo *ci = c->ci;
	bool ok = false;
	if (!ci->HasPriv(u, CA_BAN))
		source.Reply(ACCESS_DENIED);
	else if (matches_list(c, u2, CMODE_EXCEPT))
		source.Reply(CHAN_EXCEPTED, u2->nick.c_str(), ci->name.c_str());
	else if (u2->IsProtected())
		source.Reply(ACCESS_DENIED);
	else
		ok = true;

	return ok;
}

class CommandCSTBan : public Command
{
 public:
	CommandCSTBan(Module *m) : Command(m, "TBAN", 3, 3)
	{
		this->SetDesc(_("Bans the user for a given length of time"));
		this->SetSyntax(_("\037channel\037 \037nick\037 \037time\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Channel *c = findchan(params[0]);

		const Anope::string &nick = params[1];
		const Anope::string &time = params[2];

		User *u2;
		if (!c)
			source.Reply(CHAN_X_NOT_IN_USE, params[0].c_str());
		else if (!(u2 = finduser(nick)))
			source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
		else
			if (CanBanUser(source, c, u2))
			{
				Anope::string mask;
				get_idealban(c->ci, u2, mask);
				c->SetMode(NULL, CMODE_BAN, mask);
				new TempBan(dotime(time), c, mask);
				source.Reply(_("%s banned from %s, will auto-expire in %s"), mask.c_str(), c->name.c_str(), time.c_str());
			}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->OnSyntaxError(source, "");
		source.Reply(" ");
		source.Reply(_("Bans the given user from a channel for a specified length of\n"
			"time. If the ban is removed before by hand, it will NOT be replaced."));

		return true;
	}
};

class CSTBan : public Module
{
	CommandCSTBan commandcstban;

 public:
	CSTBan(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcstban(this)
	{
		this->SetAuthor("Anope");

		me = this;

		ModuleManager::RegisterService(&commandcstban);
	}
};

MODULE_INIT(CSTBan)

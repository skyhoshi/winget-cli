#include "pch.h"
#include "HomepageCommand.h"
#include "Workflows/WorkflowBase.h"
#include "Resources.h"
#include "Workflows/ManifestComparator.h"

namespace AppInstaller::CLI
{
	using namespace AppInstaller::CLI::Execution;
	using namespace std::string_view_literals;

	std::vector<Argument> HomepageCommand::GetArguments() const
	{
		return {
			Argument::ForType(Execution::Args::Type::Query),
			Argument::ForType(Execution::Args::Type::Id),
			Argument::ForType(Execution::Args::Type::Name),
			Argument::ForType(Execution::Args::Type::Moniker),
			Argument::ForType(Execution::Args::Type::Tag),
			Argument::ForType(Execution::Args::Type::Command),
			Argument::ForType(Execution::Args::Type::Source),
			Argument::ForType(Execution::Args::Type::Count),
			Argument::ForType(Execution::Args::Type::Exact),
		};
	}

	Resource::LocString HomepageCommand::ShortDescription() const
	{
		return { Resource::String::HomepageCommandShortDescription };
	}

	Resource::LocString HomepageCommand::LongDescription() const
	{
		return { Resource::String::HomepageCommandLongDescription };
	}

	void HomepageCommand::ExecuteInternal(Context& context) const
	{
		context <<
			Workflow::OpenSource <<
			Workflow::SearchSourceForSingleWithHomepage <<
			Workflow::EnsureOneMatchFromSearchResult <<
			Workflow::GetManifestFromSearchResult <<
			Workflow::ReportSearchResult;

		if (context.Contains(Execution::Data::SearchResult)) {
			const auto& searchResult = context.Get<Execution::Data::SearchResult>();
			if (searchResult.Matches.size() > 1)
			{
				context.Reporter.Warn() << "More than one result was found please refine your query. Use --Id, --Name --Tag" << std::endl;
			}
			else 
			{
				if (context.Contains(Execution::Data::Manifest)) {
					const auto& manifest = context.Get<Execution::Data::Manifest>();
					Workflow::ManifestComparator manifestComparator(context.Args);
					const auto selectedLocalization = manifestComparator.GetPreferredLocalization(manifest);
					if (selectedLocalization.Homepage.empty())
					{
						context.Reporter.Info() << "No Homepage found within manifest" << std::endl;
					}
					else {
						context.Reporter.Info() << "Homepage: " << selectedLocalization.Homepage << std::endl;
						context << Workflow::OpenHomepage;
					}
				}
				else
				{
					context.Reporter.Info() << "No Homepage found within manifest" << std::endl;
				}
			}
		}
	}
}

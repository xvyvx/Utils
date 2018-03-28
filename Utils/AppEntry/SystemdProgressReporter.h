#ifndef SYSTEMDROGRESSREPORTER_H
#define SYSTEMDROGRESSREPORTER_H

#include "IProgressReporter.h"

#if 0

#if defined(linux) || defined(__linux)

/**
 * Systemd daemon progress reporter.
 */
class SystemdProgressReporter :public IProgressReporter
{
public:
	/**
	 * Default constructor
	 */
	SystemdProgressReporter();

	/**
	 * Copy constructor(deleted)
	 */
	SystemdProgressReporter(const SystemdProgressReporter&) = delete;

	/**
	 * Move constructor
	 *
	 * @param [in,out] rhs The right hand side.
	 */
	SystemdProgressReporter(SystemdProgressReporter &&rhs) noexcept;

	/**
	 * Destructor
	 */
	virtual ~SystemdProgressReporter();

	/**
	 * Assignment operator(deleted)
	 */
	SystemdProgressReporter& operator=(const SystemdProgressReporter&) = delete;

	/**
	 * Move assignment operator
	 *
	 * @param [in,out] rhs The right hand side.
	 *
	 * @return Equal to *this.
	 */
	SystemdProgressReporter& operator=(SystemdProgressReporter &&rhs) noexcept;

	/**
	 * {@inheritDoc}
	 * 
	 * @note This is empty implementation(has no effect after calls).
	 */
	virtual void IncProgress(int step, int waitHint);

	/**
	* {@inheritDoc}
	*
	* @note Only newStatus is Status_Running/Status_StopPending has effect,other status and arguments is skipped.
	*/
	virtual void ReportNewStatus(Status newStatus, int waitHint, us32 exitCode = 0);

	/**
	* {@inheritDoc}
	*/
	virtual Status CurrentStatus()
	{
		return m_status;
	}

	/**
	 * Query this report wether create successful.
	 *
	 * @return True if create successful,false otherwise.
	 */
	operator bool()
	{
		return m_sd_notify != NULL;
	}

private:
	void *m_libHandle;  /**< Handle of libsystemd */

	int(*m_sd_notify)(int unset_environment, const char *state); /**< sd_notify pointer gets from libsystemd. */

	Status m_status;	/**< Current status. */
};

#endif

#endif

#endif /* SYSTEMDROGRESSREPORTER_H */
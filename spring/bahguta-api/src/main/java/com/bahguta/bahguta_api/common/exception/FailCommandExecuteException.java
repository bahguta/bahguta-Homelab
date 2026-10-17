package com.bahguta.bahguta_api.common.exception;

public class FailCommandExecuteException extends BahgutaException{

	/**
	 * 
	 */
	private static final long serialVersionUID = -198585227939356913L;

	public FailCommandExecuteException(String msg, Throwable cause) {
		super(msg, cause);
	}
	
	public FailCommandExecuteException(String msg) {
		super(msg);
	}

}
